  <para>At startup, &mapson; will try to read its configuration file at
<filename>${HOME}/.mapson/config</filename> first of all. If that file does not
exist, &mapson; falls back to the system-wide file
<filename>/etc/mapson/mapson.config</filename>. This means that if &mapson;
has been installed well, you don't need a configuration file of your own. It
also means, that you may <emphasis>have</emphasis> a configuration file of your
own nonetheless.</para>

  <para>The config file is read line by line. Empty lines are ignored, as are
lines starting with the commend delimiter <quote><literal>#</literal></quote>.
Anything else is supposed to start with one of the keywords listed below,
followed by one or more whitespace characters, followed by the actual data
part.</para>

  <para>The data part may contain environment variables, which you can use to
have one configuration file fit all of your users! See the sample configuration
file installed at
<filename>/usr/local/share/sample-challenge-template</filename> for such an
example.</para>

<para>Valid keywords in the configuration file are:</para>

  <variablelist>
    <varlistentry>
      <term><literal>Mailbox</literal> <parameter>file</parameter></term>
      <listitem>
        <para>This directive sets the complete path of the mailbox file, where
&mapson; stores approved mails. Unless, of course, the parameter configured here
starts with a pipe sign (<quote><literal>|</literal></quote>), as in
<quote><literal>|/usr/sbin/sendmail foo@example.org</literal></quote>. In this
case, &mapson; will pipe the mail to the standard input channel of this command
rather than to write it to a file.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>SpoolDir</literal> <parameter>directory</parameter></term>
      <listitem>
        <para>This directive sets the complete path to the directory, in which
deferred mails will be spooled until a confirmation arrives for them.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>AddressDB</literal> <parameter>file</parameter></term>
      <listitem>
        <para>This directive sets the complete path of the file &mapson; uses to
store the <quote>known</quote> addresses.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>WhiteListDB</literal> <parameter>file</parameter></term>
      <listitem>
        <para>The <quote>white list</quote> is a list of regular expressions,
one per line. E-Mail coming from an address matching any of those regular
expressions will always pass &mapson;. Be careful what you add here! The default
location is <filename>${HOME}/.mapson/whitelist-db</filename>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>WhoamiDB</literal> <parameter>file</parameter></term>
      <listitem>
        <para>HashCash cookies are issued for the recipient's address, hence,
&mapson; must know your local addresses in order to verify the HashCash is
valid. This option sets the path to an address database very much like
<literal>AddressDB</literal>, which should contain one e-mail address per line.
HashCashes issued for any of these addresses are accepted, everything else is
ignored. The default location is
<filename>${HOME}/.mapson/whoami-db</filename>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>HashCashDB</literal> <parameter>file</parameter></term>
      <listitem>
        <para>The complete path the seen-HashCash database. The file will be #
created if it does not exist. The default location is
<filename>${HOME}/.mapson/hashcash-db</filename>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>HashCashExpiry</literal> <parameter>seconds</parameter></term>
      <listitem>
        <para>HashCash cookies contain a time stamp, which tells, when they were
generated. Per defaultt, &mapson; will ignore all HashCashes, which are older
than a week (604800 seconds). You can change this setting here.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>HashCashGrace</literal> <parameter>seconds</parameter></term>
      <listitem>
        <para>Set a grace time allowing for clock skew. This setting is used,
for example, when a HashCash cookie arrives with a timestamp in the future! If
the error is within the grace time, it will be accepted nonetheless. The default
is one hour (3600 seconds).</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>ReqHashCashBits</literal> <parameter>Integer</parameter></term>
      <listitem>
        <para>How large a bit-collision do we require in order to accept a
HashCash as valid. The default is to require 20 bits.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>AddressDBAutoAdd</literal> <parameter>boolean</parameter></term>
      <listitem>
        <para>If this directive is set to <literal>true</literal> (the default), then &mapson; will add so-far unknown addresses from which mail has been accepted to the database automatically, so that in future mails from these addresses will pass. You can disable this behavior, though, in case you want to maintain that database manually or by other means.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>ChallengeTemplate</literal>
<parameter>file</parameter></term>
      <listitem>
        <para>This directive sets the complete path to the
challenge template file &mapson; uses to generate the
challenge mail sent to first-time originators.</para>

        <para>An arbitrary number of alternate paths can be specified, if
they're separated by colons, for example:</para>

        <informalexample>
          <screen>$HOME/.mapson/challenge-template:$DATADIR/challenge-template:&hellip;</screen>
        </informalexample>

        <para>In this setup, &mapson; would first try to load the file
<filename>$HOME/.mapson/challenge-template</filename>. If that failed, it would
try <filename>$DATADIR/challenge-template</filename>, and so on, until one of the
files can be loaded successfully.</para>

        <para>This is an extremely useful feature if you are a system
administrator who wishes to allow all users of the system to use &mapson;
without having to create a challenge template of their own:
Configure &mapson; to load that challenge template first, that is
located in the user's home directory. If this file does not exist, then fall
back to the system-wide file.</para>

        <para>In effect, that means that the user can simply use &mapson; to
filter his mail, and if he ever feels like it, he can create a
request-for-confirmantion template file of his own and it will be preferred over
the system-wide one.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>MTA</literal> <parameter>command</parameter></term>
      <listitem>
        <para>This directive sets the command &mapson will use to send-out a
challenge mail. The actual mail will be piped into the started
process.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>PassIncorrectMails</literal>
<parameter>boolean</parameter></term>
      <listitem>
        <para>When &mapson; parses the incoming mail's headers for the
addresses, it may detect syntax errors in the mail header, that do not cause a
fatal error, but that surely hint to the fact that this mail was not created by
an RFC822-conformant mail client.</para>

        <para>Many spam mails contain incorrect header lines, so you may chose
to have &mapson fail on <emphasis>any</emphasis> syntax error -- even non-fatal
ones. <quote>Failing</quote> means that &mapson; will abort and return the
return code configured below to the <acronym>MTA</acronym>. Depending on the
setting of the return code, the MTA will then bounce the mail.</para>

        <para>The parameter given to this option is a boolean, meaning that you
may specify either <literal>yes</literal> or <literal>no</literal>.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>RuntimeErrorRC</literal>
<parameter>integer</parameter></term>
      <listitem>
        <para>This directive sets the return code &mapson; exits with in case it
had to abort with a run-time error. Possible run-time errors are failure to open
file, lack of available memory, <abbrev>etc.</abbrev> &hellip;</para>

        <para>The default choice is <quote>75</quote>, which
<application>sendmail</application> will interpret as a temporary system error,
so it will queue the mail and re-try.</para>

        <para>A valid return code is a positive integer up to 128.</para>
      </listitem>
    </varlistentry>
    <varlistentry>

      <term><literal>SyntaxErrorRC</literal>
<parameter>integer</parameter></term>
      <listitem>
        <para>This directive sets the return code &mapson; exits with in case it
encountered a fatal syntax error in the e-mail. If
<symbol>PassIncorrectMails</symbol> is disabled, non-fatal syntax errors will
also cause &mapson; to abort with this return code.</para>

        <para>The default choice is <quote>65</quote>, which
<application>sendmail</application> will interpret as a permanent error that
causes the mail to bounce.</para>

        <para>A valid return code is a positive integer up to 128.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>Debug</literal> <parameter>boolean</parameter></term>
      <listitem>
        <para>If you enable debugging messages by saying <literal>yes</literal>
here, &mapson; will log additional information about its procssing of the mail.
If you say <literal>no</literal>, &mapson; will log only very few messages at
all.</para>

        <para>Debugging is available only when the binary has been compiled with
the <symbol>DEBUG</symbol> symbol defined. Currently, that is the default,
though, so unless you exclicitely disabled it, debugging will be
available.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><literal>LogFile</literal> <parameter>file</parameter></term>
      <listitem>
        <para>This directive sets the complete path of the file &mapson; uses to
log its actions.</para>
      </listitem>
    </varlistentry>
  </variablelist>

  <para>In order to make the contents of the configuration file as independent
from the system's directory structure as possible, &mapson; provides a set of
environment variables, which are guaranteed to be defined. You can use them
anywhere in the data part of a configuration directive, and you can use the
usual manipulations on them.</para>

  <para>Environment variables are looked-up case-sensitively, so
<varname>$home</varname> is not the same thing as <varname>$HOME</varname>. This
behavior is different in the challenge template, where you can
spell the variables upper- or lower-case as you wish. That's because the
variables there are not coming from the environmnet, but are &mapson;'s internal
variables. So be sure not to confuse that, because an undefined variable in this
file will cause &mapson; to abort with an error.</para>

  <para>Here is the complete list:</para>

  <variablelist>
    <varlistentry>
      <term><varname>$MAILBOXDIR</varname></term>
      <listitem>
        <para>This variable contains the complete path of directory, in which
the system's mailboxes are located, usually
<filename>/var/spool/mail</filename>. Please note that the value provided here
is the one determined at <emphasis>compile-time</emphasis>, so if you changed
your system's installation and want to rely on this variable, you'll have to
re-compile.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><varname>$MTA</varname></term>
      <listitem>
        <para>This variable contains the path to the systems mail transport
agent. Please note, that this is only the path of the executable -- for example
<filename>/usr/sbin/sendmail</filename> --, the variable does not contain the
flags that must be passed to the <acronym>MTA</acronym> in order to do something
useful.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><varname>$DATADIR</varname></term>
      <listitem>
        <para>This variable contains the complete path of the directory, which
has been compiled into &mapson; as the directory where read-only
architecture-independent data should be stored. You will, for example, find the
system-wide challenge template file here.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><varname>$USER</varname></term>
      <listitem>
        <para>This variable contains the name of the user under which &mapson;
is running. Depending on your <acronym>MTA</acronym>, this must not necessarily
be the user who is receiving mail! If you're using
<application>sendmail</application>, though, you're on the secure side.</para>
      </listitem>
    </varlistentry>

    <varlistentry>
      <term><varname>$HOME</varname></term>
      <listitem>
        <para>This variable contains the complete path of
<varname>$USER</varname>'s home directory.</para>
      </listitem>
    </varlistentry>
  </variablelist>


<!--
Local Variables:
mode: sgml
fill-column:80
sgml-parent-document:("mapson.sgml" "article" "sect1")
End:
-->
