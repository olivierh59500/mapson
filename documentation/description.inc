  <para>The amount of unsolicited commercial e-mail ("spam") circulating in the
Internet today has become unbearable for most people. Many approaches have been
proposed to stop this junk from filling up your mailboxes, such as the <ulink
url="http://mail-abuse.org/rbl+/">Real-time blackhole list</ulink>, <ulink
url="http://www.iks-jena.de/mitarb/lutz/usenet/teergrube.en.html">Teergruben</ulink>,
or <application>procmail</application>-based <ulink
url="http://junkfilter.zer0.org/">anti-spam recipts</ulink>.</para>

  <para>&mapson; is another anti-spam system, but it uses an approach entirely
different than those systems named before. Instead of trying to recognize spam
by the <acronym>IP</acronym> address of the <acronym>SMTP</acronym> dialag's
peer or by certain patters in the mail's body, &mapson; uses the sender's e-mail
address to decide whether the e-mail is delivered to your mailbox or not: Any
e-mail that comes from a "known" address may pass, any e-mail that comes from an
e-mail address seen for the first time needs special confirmation, before it may
pass.</para>

  <para>"Special confirmation" means that &mapson; will generate an
<acronym>MD5</acronym> checksum of the to-be-confirmed mail and stores the mail
in a temporary spool directory. Then it sends an <firstterm>request for
confirmation</firstterm> to the address from which the mail was coming from. In
this request, it will include the <acronym>MD5</acronym> checksum and ask the
recipient to reply back and to quote that <acronym>MD5</acronym> hash. Once
&mapson; sees that <acronym>MD5</acronym> hash again, it considers that a
confirmation of the original mail, delivers the deferred mail from the spool to
your mailbox, and adds the sender's address to the database of known addresses,
so that the next time he tries to contact you, his mail will pass through
&mapson; immediately..</para>

  <para>This heuristic catches almost any spam mail, because spammers have to
fake their sender addresses in order to avoid being held responsible for their
abuse. Hence, their address will most likely not be in the database of known
addresses, nor will they ever receive the request for confirmation mail!</para>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("mapson.sgml" "article" "sect1")
End:
-->
