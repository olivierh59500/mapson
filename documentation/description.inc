  <para>&mapson; is a spam filter that uses a pretty unique approach to keep
unsolicited commercial e-mail out of your mailbox. Rather than using a set of
configured "bad words", a list of "know spammers", or complicated scoring
mechanisms to determine what is spam and what is not, it relies on "known
senders" -- or rather "unknown senders".</para>

  <para>Every time you receive an e-mail, &mapson; will look-up the sender's
e-mail address in a small database file and check whether that address is in
there. If it is, the mail is delivered to your mailbox, but if it is not, the
e-mail will be stored in a spool directory in your home, using a cryptographic
cookie as the filename. Then &mapson; will send a so called <firstterm>request
for confirmation</firstterm> to the sender's address, asking him to please
confirm his addresses validity by replying and sending the cryptograpic cookie
back. When &mapson; receives a mail with such a cookie in it, it will move the
corresponding mail from the spool directory to your mailbox and add the sender's
address in the mail to the database.</para>

  <para>This approach is based on the fact that spammers usually fake the sender
address of the spam mail. (In fact, they <emphasis>have to</emphasis>, because
sending unsolicited advertisement via e-mail is illegal in most countries.) But
because their sender address is invalid, they will never see the request for
confirmation, they will never reply, and their spam will sit in that spool file
until hell freezes over or an apropriate cron job deletes it. Using this
heuristic, &mapson; catches way above 95% of all spam mail I receive.</para>

  <para>In order to avoid annoying more "real" people, who are trying to contact
you, than necessary, you can import the addresses from your mail archive into
the &mapson; database. Furthermore, you can set &mapson; up in a way that will
let any mail pass automatically, that is a reply to a mail or a news posting of
yours: If you sent someone an e-mail and he replies back, &mapson; won't bother
him. It would be pretty inpolite, if it did.</para>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("mapson.sgml" "article" "sect1")
End:
-->
