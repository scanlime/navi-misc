from django.contrib.sessions.models import Session
import datetime


class TokenClass:
    """Tokens are specialized sessions- they are opaque
       and unguessable identifiers that can be given to
       clients in order to identify some securely stored
       server-side state.
       """
    def __init__(self, key, expire_hours=2):
        self.key = key
        self.expiration = datetime.timedelta(hours=expire_hours)

    def new(self, data={}):
        token = Session.objects.get_new_session_key()

        d = {self.key: True}
        d.update(data)

        Session.objects.save(token, d, datetime.datetime.now() + self.expiration)
        return token

    def get(self, token):
        """Returns a dictionary with the token data on success. If the
           token is invalid or expired, returns None.
           """
        try:
            session = Session.objects.get(session_key=token)
        except Session.DoesNotExist:
            return None

        if session.expire_date < datetime.datetime.now():
            return None

        decoded = session.get_decoded()
        if not decoded.get(self.key):
            return None
        
        return decoded

    def delete(self, token):
        Session.objects.save(token, None, datetime.datetime.now())
