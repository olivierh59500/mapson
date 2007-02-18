  <para>&librfc; provides application developers with a complete parser for
RFC822 addresses. Not only can you use the library to verify that RFC822
addresses are syntactically correct, you can also have an address split up into
its semantic parts, what is needed when deciding where to route an address to,
etc.</para>

  <para>What is quite unique is that &librfc; does indeed parse
<emphasis>all</emphasis> address types allowed by the standard. That includes
such weird things as <quote>address groups</quote> or addresses with whitespace
and comments throw in. Take a look at this beast to get an idea:</para>

  <informalexample>
    <screen>testing my parser : peter.simons@gmd.de,
        (peter.)simons@rhein.de ,,,,,
     testing my parser &lt;simons@ieee.org&gt;,
     it rules &lt;@peti.gmd.de,@listserv.gmd.de:simons @ cys .de&gt;
     ;
     ,
     peter.simons@acm.org</screen>
  </informalexample>

  <para>That is indeed a legal e-mail address in RFC822 messages. It contains
five separate addresses, which are grouped together. Here's the parsed
result:</para>

  <informalexample>
    <screen>peter.simons@gmd.de
simons@rhein.de
simons@ieee.org
&lt;@peti.gmd.de,@listserv.gmd.de:simons@cys.de&gt;
peter.simons@acm.org</screen>
  </informalexample>

  <para>In case you wonder: The strange looking address that's listed fourth is
a so called <quote>routing address</quote> -- and yes, that's a legal e-mail
address, too. These were popular in the early days of the Internet. Back then,
every mail server that relayed an e-mail put its own address into this construct
so that bounces could be routed the same way back that they originally went. The
address says that the mail should be send to the host
<literal>peti.gmd.de</literal>, then to <literal>listserv.gmd.de</literal>, and
from there it should be delivered (using any route) to the address
<literal>simons@cys.de</literal>. These days, such addresses can hardly be used,
because nobody relays for other recipients anymore. Still, these are
legal.</para>

  <para>&librfc; provides you with several routines that parse the different
flavours of e-mail addresses as defined in the standard. The results will be
placed in a <type>rfc822address</type> structure and returned. If constructs are
parsed that may contain multiple addresses, you can pass a
<quote>committer</quote> class to the function, which is called every time a
correct address is found and may append it to a container of your choice.</para>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("user-manual.sgml" "article" "sect1")
End:
-->
