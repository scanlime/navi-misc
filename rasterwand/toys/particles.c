/*
 * Simple particle system demo for the Rasterwand
 *
 * -- Micah Dowty <micah@navi.cx>
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>
#include <rwand_dev.h>

struct particle
{
    float x, y;
    float vx, vy;
    int visible;
    float lifetime;
};

int rwand_open(const char *filename)
{
    int fd;
    fd = open(filename, O_RDWR);
    if (fd < 0) {
	perror("open");
	exit(1);
    }
    return fd;
}

void rwand_init(int fd)
{
    struct rwand_settings settings;
    ioctl(fd, RWANDIO_GET_SETTINGS, &settings);
    settings.display_center = 0xFFFF * 0.5;
    settings.display_width = 0xFFFF * 0.6;
    settings.duty_cycle = 0xFFFF * 0.2;
    ioctl(fd, RWANDIO_PUT_SETTINGS, &settings);
}

void rwand_update(int fd, struct particle *particles,
		  int num_particles, int columns)
{
    static unsigned char frame[RWAND_FB_BYTES];
    int i;

    memset(frame, 0, columns);
    for (i=0; i<num_particles; i++) {
	struct particle *p = &particles[i];
	if (p->visible) {
	    int ix = p->x + 0.5;
	    int iy = p->y + 0.5;

	    if (ix < 0 || ix >= columns ||
		iy < 0 || iy >= 8) {
		p->visible = 0;
		continue;
	    }
	    frame[ix] |= 1 << iy;
	}
    }

    write(fd, frame, columns);
}

float uniform_variate(float min, float max)
{
    return (((float)random()) / RAND_MAX) * (max-min) + min;
}

void particle_init(struct particle *p)
{
    float angle, speed;

    p->x = 20;
    p->y = 3.5;
    p->visible = 1;

    speed = uniform_variate(5, 30);
    angle = uniform_variate(0, M_PI * 2);

    p->vx = speed * cos(angle);
    p->vy = speed * sin(angle);

    p->lifetime = uniform_variate(1.5, 4.0);
}

void particle_update(struct particle *p, float dt)
{
    p->lifetime -= dt;
    if (p->lifetime <= 0) {
	particle_init(p);
	return;
    }
    if (p->visible) {
	p->x += p->vx * dt;
	p->y += p->vy * dt;
    }
}

float get_time_step()
{
    static struct timeval then;
    static int then_isvalid;
    struct timeval now;
    float dt;

    gettimeofday(&now);

    if (then_isvalid)
	dt = (now.tv_sec - then.tv_sec) +
	    (now.tv_usec - then.tv_usec) / 1000000.0;
    else
	dt = 0;

    then = now;
    then_isvalid = 1;
    return dt;
}

void particle_loop(int fd, int num_particles) {
    struct particle *particles;
    int i;
    particles = malloc(sizeof(struct particle) * num_particles);

    for (i=0; i<num_particles; i++)
	particle_init(&particles[i]);

    while (1) {
	float dt = get_time_step();
	for (i=0; i<num_particles; i++)
	    particle_update(&particles[i], dt);
	rwand_update(fd, particles, num_particles, 40);
    }

    free(particles);
}

int main() {
    int fd = rwand_open("/dev/rwand0");
    rwand_init(fd);
    particle_loop(fd, 100);
    return 0;
}

/* The End */
