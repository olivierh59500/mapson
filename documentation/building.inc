    <para>Compiling the software should be pretty straight forward; all you'll
have to do is the usual

      <screen>
    ./configure
    make
    make install</screen>

routine. The last step may require super-user privileges, so have the
<systemitem class="username">root</systemitem> password ready if you're going
for a system-wide installation.</para>

    <para>Be adviced that &mapson; needs a fairly recent C++ compiler because it
make full use of the new ISO C++ language features. If you're using the <ulink
url="http://gcc.gnu.org/">GNU C Compiler</ulink> version 2.95 or later, you
won't have any problems. But other compilers are known not to be ISO C++
compatible. If you're having trouble, send me an e-mail and I'll see what I can
do.</para>

    <para>The <filename>configure</filename> script is a standard <ulink
url="http://www.gnu.org/software/autoconf/">GNU Autoconf</ulink> script, which
supports all the usual options. If you are not familiar with these scripts,
please refer to the "Running <filename>configure</filename> Scripts" section of
the <application>Autoconf</application> user manual, which is available at:

    <informalfigure>
        <ulink url="http://www.gnu.org/manual/autoconf/html_chapter/autoconf_13.html#SEC129">http://www.gnu.org/manual/autoconf/html_chapter/autoconf_13.html#SEC129</ulink>
      </informalfigure>
    </para>

    <para>The following list will show only those options that are
specific to &mapson;.</para>

    <variablelist>
      <varlistentry>
        <term><option>--with-mailboxdir=<replaceable>DIR</replaceable></option></term>
        <listitem>
          <para>In order to deliver incoming e-mail to your mailbox, &mapson
needs to know where the user mailboxes are located on your system. It tries to
figure that out automatically by checking for the existance of the directories
<filename>/var/mail</filename> and <filename>/var/spool/mail</filename>, but
depending on your setup, you might want to choose another path here.</para>
          </listitem>
      </varlistentry>

      <varlistentry>
        <term><option>--with-mta=<replaceable>PATH</replaceable></option></term>
        <listitem>
          <para>&mapson; needs to know the complete path to your system's
<systemitem>mail transport agent</systemitem> (<abbrev>MTA</abbrev>) in order to
send out requests for confirmation. The <filename>configure</filename> script
will assume that you have <application>sendmail</application> installed and look
for it in various locations, but you can (and may have to) set the right choice
manually using this option.
          </listitem>
      </varlistentry>

      <varlistentry>
        <term><option>--with-debug</option></term>
        <listitem>
          <para>Per default, &mapson; comes with a couple of additional debug
messages, which you can enable on the command line or in the configuration file
if you feel something's going wrong. But in order for these log messages to be
available, the binary must have been compiled with the <symbol>DEBUG</symbol>
flag. Using this option, you can manually decide whether you want these log
messages compiled in or not.</para>
        </listitem>
      </varlistentry>
    </variablelist>

    <para>Any of the paths you configure here are only used as defaults. You can
override them at run-time in the configuration file &mapson; reads at
startup.</para>
