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
      <term><option>-c <replaceable>config</replaceable></option></term>
      <term><option>--config-file <replaceable>config</replaceable></option></term>
      <listitem>
        <para>Use the configuration file
<filename><replaceable>config</replaceable></filename> rather the
default.</para>
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
