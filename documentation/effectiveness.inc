  <para>You may wonder how effective &mapson; is -- and rightfully so. Of
course, being the principal author of this tool, I am biased. But I tried my
best to conduct an objective study, determining how many spam mails have been
caught, how wany &mapson; did not catch, and how many mails were delayed that
weren't spam. The numbers presented below are somewhat skewed, because the
logfile I analyzed includes the various test mails I piped into &mapson; in
order to test and to debug it, but if at all, they tend to make the result look
worse because I did not test the case that a mail comes in and is approved, I
tested the case that a mail is <emphasis>deferred</emphasis>.</para>

  <para>Anyway, I activated &mapson; for my private mail account on January
11th, 2002. This analysis was made on April 10th the same year, so we have a
test period of 90 days -- almost three months. Here are the numbers:</para>

  <informaltable colsep="1" pgwide="0">
    <tgroup cols="2">
      <tbody>
        <row>
          <entry>Address Database entries, imported:</entry>
          <entry>4407</entry>
        </row>
        <row>
          <entry>Address Database entries, today:</entry>
          <entry>4606</entry>
        </row>
        <row>
          <entry>Total number of mails received:</entry>
          <entry>7632</entry>
        </row>
        <row>
          <entry>Mails coming from mailing lists:</entry>
          <entry>2104</entry>
        </row>
        <row>
          <entry>Number of processed mails:</entry>
          <entry>1561</entry>
        </row>
        <row>
          <entry>Number of passed mails:</entry>
          <entry>395</entry>
        </row>
        <row>
          <entry>Number of mails deferred:</entry>
          <entry>998</entry>
        </row>
        <row>
          <entry>Number of mails confirmed:</entry>
          <entry>67</entry>
        </row>
        <row>
          <entry>Number of mails unacknowledged <quote>real</quote> mails:</entry>
          <entry>14</entry>
        </row>
        <row>
          <entry>Number of definite junk mails:</entry>
          <entry>862</entry>
        </row>
      </tbody>
    </tgroup>
  </informaltable>

  <para>How did I get those numbers? First of all, I counted all logfile entries
coming from <application>sendmail</application>, which contained the strings
<quote>mailer=local</quote> and <quote>to=&lt;simons</quote>. This I assumed to be
the number of total mails received. On those entries, I counted how many of
those contained the string <quote>to=&lt;simons+</quote> -- what should largely
be the mails I receive via mailing lists because I use
<application>procmail</application>'s argument feature when I subscribe to
mailing lists.</para>

  <para>All those mails, addressed to <quote>simons+something</quote> bypassed
&mapson; automatically. Furthermore, all mails that contained certain headers
like <literal>In-Reply-To</literal> or <literal>References</literal> bypassed
&mapson; entirely because I assume those to be replies to messages of mine. This
explains why the number of mails that &mapson; actually processed is much
smaller than the number of total mails received: Apparently only 20% of all
incoming e-mail were processed by &mapson; at all!</para>

  <para>Then I counted the entries in &mapson;'s logfile that sayed
<quote>passed</quote>, which turned out to be 25% of all mails &mapson; saw. The
mails that &mapson; did not let through amount to 63% of all mails processed by
&mapson;, and I determined those by counting the logfile entries that said
<quote>Spooling e-mail</quote>.</para>

  <para>Of those deferred e-mails, only 6% were confirmed later! So then I waded
through the spool directory and moved all <quote>regular</quote> e-mails to a
separate folder and counted them: It turned out that regular 14 mails were
<emphasis>not</emphasis> confirmed by the sender, that's 0.14% of all deferred
mails and 0.0018% of the total number of mails received.</para>

  <para>When I looked at those mails in detail, it turned out that 3 of the 14
mails lying in the spool <quote>unwarrantedly</quote> actually
<emphasis>had</emphasis> been acknowledged, but that the other person was too
dumb to reply correctly: Two of the three sent a <emphasis>new</emphasis>
e-mail, which did of course not contain the cookie, and the third person replied
to the request for confirmation, but erased the cookie from the mail manually.
It's no wonder that the mails they sent me turned out to be of the kind that I
don't want anyway.</para>

  <para>The remaining 11 mails that were not delivered to me but were not spam
either, were some kind of replies I got from Internet sites like amazon.com,
other customer service stuff and automated reminders. No personal e-mail. They
were delayed because they were sent with incorrect sender addresses that bounced
when &mapson; sent the request for confirmation back or that were apparently not
read by the other end at alll.</para>

  <para>Once I noticed the problem, I got it fixed quickly by using
<quote>user+something</quote> addresses at sites like amazon.com, too, so that
their mails bypass &mapson; to begin with.</para>

  <para>Of course there was a certain amount of spam that got past &mapson; one
way or the other. Some used addresses that actually were in my database because
I had imported them from my mail archives when I set &mapson; up. Some others
were routed past &mapson; by my <application>procmail</application>
configuration because they looked like bounces or postings coming from mailing
lists. Unfortunately, I cannot determine any exact number without wading through
my mail archive manually, and I honestly don't want to do that. After all, the
whole point of writing &mapson; was that I do <emphasis>not</emphasis> have to
see spam!</para>

  <para>To summarize: In the 90 days of testing, &mapson; caught 862 definite
spam mails: That's about 10 per day. It reduced the amount of spam in my mailbox
to one or two mails once a week. At the same time, only 0.008 percent of all
e-mail I receive had to be acknowledged, what I think is an acceptable level of
inconvenience for my communication partners, especially given the fact that
obviously they were contacting <emphasis>me</emphasis>, not the other way
round.</para>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("mapson.sgml" "article" "sect1")
End:
-->
