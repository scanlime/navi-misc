/* From RFC1831 */

   program PING_PROG {
         /*
          * Latest and greatest version
          */
         version PING_VERS_PINGBACK {
            void
            PINGPROC_NULL(void) = 0;

            /*
             * Ping the client, return the round-trip time
             * (in microseconds). Returns -1 if the operation
             * timed out.
             */
            int
            PINGPROC_PINGBACK(void) = 1;
         } = 2;

         /*
          * Original version
          */
         version PING_VERS_ORIG {
            void
            PINGPROC_NULL(void) = 0;
         } = 1;
      } = 1;

      const PING_VERS = 2;      /* latest version */
