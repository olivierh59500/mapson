/*
 * Copyright (c) 1998-2002 by Peter Simons <simons@cryp.to>.
 * All rights reserved.
 */

#include "rfc822.hpp"
using namespace std;

// This a custom committer class, which is used by the rfc parser to
// store the parsed addresses into whatever the container wishes to
// use. This is already rather sophisticated, so don't be scared. Just
// look at the rest of the demo program -- it is really simple, you'll
// see.

class my_committer : public rfc822parser::address_committer
    {
  public:
    void operator() (const rfc822address & addr) { cout << addr << endl; }
    };

// The test program will read an rfc address from standard input and
// parse it several times, demonstrating the various techniques you
// can use. A good input to test the parser with would be the
// following:
//
// testing my parser : peter.simons@gmd.de, (peter.)simons@rhein.de ,,,,,
//      testing my parser <simons@cryp.to>,
//      it rules <@peti.gmd.de,@listserv.gmd.de:simons @ cys .de>
//      ;
//      ,
//      peter.simons@acm.org

int
main()
try {
#if 0
    // Read one address line as defined rfc822 from standard input
    // into a string buffer.

    string line, buffer;
    for (getline(cin, line); cin; getline(cin, line))
	{
	buffer += line;
	buffer += "\n";
	line = "";
	}

    // Parse all addresses without storing the result anywhere, just
    // to check whether they're syntactically correct.

    check_rfc822_addresses(buffer);

    // Parse all addresses and store the result in a deque container.

    deque<rfc822address> deq;
    insert_iterator< deque<rfc822address> > ii(deq, deq.end());
    parse_rfc822_addresses(&ii, buffer);
    cout << "Found " << deq.size() << " separate addresses." << endl;


    // Use a custom address_committer class to print all parsed
    // addresses to the standard output stream.

    my_committer   committer;
    rfc822parser parser(lex(buffer), &committer);
    parser.addresses();
#endif


    // Do a few other simple tests, just to see whether any of the
    // glue routines is behaving strangely.

    check_rfc822_addresses   ("simons@example.org");
    check_rfc822_mailboxes   ("simons@example.org");
    check_rfc822_address     ("simons@example.org");
    check_rfc822_mailbox     ("simons@example.org");
    check_rfc822_addr_spec   ("simons@example.org");
    check_rfc822_mailbox     ("<simons@example.org>");

    cout << parse_rfc822_mailbox     ("simons@example.org")   << endl;
    cout << parse_rfc822_addr_spec   ("simons@example.org")   << endl;
    cout << parse_rfc822_mailbox     ("<simons@example.org>") << endl;

    // Done.

    return 0;
    }
catch(rfc822_syntax_error & e)
    {
    cout << "Address contains an syntax error: "
	 << e.what()
	 << endl;
    }
catch(exception & e)
    {
    cout << "Caught exception: "
	 << e.what()
	 << endl;
    }
catch(...)
    {
    cout << "Caught unknown exception." << endl;
    }
