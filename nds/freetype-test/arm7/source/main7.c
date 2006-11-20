#include <nds.h>
#include <loader-exit.h>

void fifo_irq_handler()
{
    uint32 message = REG_IPC_FIFO_RX;
    switch (message) {

    case IPC_MSG_LOADER_EXIT:
	Loader_Exit7();
	break;

    }
}

void vblank_irq_handler()
{
    IPC->buttons = REG_KEYXY;
    IPC->heartbeat++;
}

int main()
{
    irqInit();
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

    irqSet(IRQ_VBLANK, vblank_irq_handler);
    irqEnable(IRQ_VBLANK);
    irqSet(IRQ_FIFO_NOT_EMPTY, fifo_irq_handler);
    irqEnable(IRQ_FIFO_NOT_EMPTY);

    while (1) {
	swiWaitForVBlank();
    }
}

