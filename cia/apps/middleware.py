class BehindReverseProxy(object):
    """Middleware for Django installations running behind a reverse
       proxy. Sets the request's address from the X-Forwarded-For header,
       and sets is_secure() according to the X-Forwarded-Proto header.

       NB: This is similar to Django's SetRemoteAddrFromForwardedFor,
           with the addition of HTTP support. This also includes a
           bugfix.  The original implementation in Django would take
           the first IP address. However by convention, proxy servers
           augment existing X-Forwarded-For headers by appending their
           own data to the end. This means that, assuming we're
           running behind a single trusted proxy, the IP we're looking
           for is the last one in what might be a comma-separated list
           of several IPs.
       """
    def process_request(self, request):
        try:
            real_ip = request.META['HTTP_X_FORWARDED_FOR']
        except KeyError:
            return

        real_ip = real_ip.split(",")[-1].strip()
        request.META['REMOTE_ADDR'] = real_ip

        if request.META.get('HTTP_X_FORWARDED_PROTO', '').find('https') >= 0:
            request.is_secure = lambda: True
