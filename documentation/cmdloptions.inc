  <variablelist>
    <varlistentry>
      <term><option>-h</option></term>
      <term><option>--help</option></term>
      <listitem>
        <para>Show &mapson; usage information.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><option>--version</option></term>
      <listitem>
        <para>Show &mapson;'s version string.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><option>-d</option></term>
      <term><option>--debug</option></term>
      <listitem>
        <para>Enable debugging. Please note that debugging is only available if
&mapson; has been compiled with the define <symbol>DEBUG</symbol>. Otherwise,
the debug code is not included in the binary.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><option>-a</option></term>
      <term><option>--accept</option></term>
      <listitem>
        <para>Accept the incoming e-mail unconditionally and add the sender's
addresses to the database.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><option>--cookie</option> <replaceable>cookie</replaceable></term>
      <listitem>
        <para>Using this parameter, you can specify a cookie on the command
line. &mapson; will then try to approve the corresponding mail from the spool.
If the cookie turns out to be incorrect, &mapson; will continue to process the
mail as if none had been specified. That means, though, that if a valid cookie
is found in the mail itself, it will approve the corresponding mail
nonetheless.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><option>-c <replaceable>config</replaceable></option></term>
      <term><option>--config-file <replaceable>config</replaceable></option></term>
      <listitem>
        <para>Use the configuration file
<filename><replaceable>config</replaceable></filename> rather the
default.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><option>--dont-scan</option></term>
      <listitem>
        <para>Do not scan for cookies in the incoming e-mail. This is useful in
case you're using procmail (or some similar mechanism) to direct cookies to
special addresses and thus can use the --cookie option rather than to have
mapSoN look through the mail for one.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><option><replaceable>mail &hellip;</replaceable></option></term>
      <listitem>
        <para>If any parameter is specified on the command line that is not an
option, &mapson; will go into <firstterm>gather addresses</firstterm> mode. The
parameters are interpreted as filenames, each of the files containing an e-mail
that &mapson; will parse. Any sender address &mapson; finds in these mails will
be added to the database of known addresses. This mode is meant to import
addresses from your mail archive to the database.</para>
      </listitem>
    </varlistentry>
  </variablelist>

<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("mapson.sgml" "article" "sect1" "title" "para")
End:
-->
