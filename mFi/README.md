# snippets

metrics.cgi
===========

Metrics.cgi exports mFi outlet data to prometheus via a /metrics URL.

To use this, copy metrics.cgi to your mFi at /etc/persistent/bin.

Also copy /usr/www/mfi/hw.cgi into /etc/persistent/bin.

Make the following edits to /etc/lighthttpd.conf:

Among the other $SERVER entries, add:

    $SERVER["socket"] == ":81" {
      server.document-root = "/etc/persistent/bin"
    }

And in the "airos.allow" stanza, add:

    , "/metrics.cgi"
    , "/metrics"



Then restart lighthttpd and if that works, type "save"
to commit the script and httpd changes to flash.
