  <para>There are two minor changes in the &mapson; config file, which make it
necessary to update your installation before you can use the version 3 or later
binary. Make the following changes in <filename>$HOME/.mapson/config</filename>
(or wherever the file is on your system):</para>

  <orderedlist>
    <listitem>
      <para>The cumbersome <literal>ReqConfirmTemplate</literal> option in the
configuration file has been renamed to <literal>ChallengeTemplate</literal>. You
should edit your configuration files accordingly, because mapSoN will abort with
an error when finding the old keyword.</para>

      <para>Along with this change, the template file has been renamed from
<filename>reqmail.template</filename> to
<filename>challenge-template</filename>. Since your existing configuration files
points to the old name, you don't need to change that if you don't want
to.</para>
    </listitem>

    <listitem>
      <para>The <literal>StrictRFCParser</literal> option has been removed from
mapSoN altogether, so you'll have to remove it from your configuration files as
well.</para>
    </listitem>
  </orderedlist>

  <para>In addition to those changes, the following options are
<emphasis>new</emphasis> in the 3.x version: <literal>WhiteListDB</literal>,
<literal>WhoamiDB</literal>, <literal>HashCashDB</literal>,
<literal>HashCashExpiry</literal>, <literal>HashCashGrace</literal>, and
<literal>ReqHashCashBits</literal>. You'll find them described in the User's
Manual.</para>

  <para>One new addition in version 3 is support for HashCash. &mapson; supports
it insofar as that it let's mails with a valid HashCash always pass. Please
refer to the <ulink url="http://hashcash.org/">HashCash website</ulink> for
further details.</para>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("mapson.sgml" "article" "sect1")
End:
-->
