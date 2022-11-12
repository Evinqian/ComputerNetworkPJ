import re, os, sys, time, traceback
from subprocess import check_call, check_output, PIPE, Popen
from optparse import OptionParser

TOTAL = POSSIBLE = 0
CURRENT_TEST = None
TESTS = []

def test(points, title):
    def register_test(fn, title=title):
        global TOTAL, POSSIBLE
        def run_test():
            global TOTAL, POSSIBLE, CURRENT_TEST

            # Handle test dependencies
            if run_test.complete:
                return
            run_test.complete = True

            # Run the test
            fail = None
            start = time.time()
            CURRENT_TEST = run_test
            sys.stdout.write("%s: " % title)
            sys.stdout.flush()
            try:
                fn()
            except AssertionError as e:
                fail = "".join(traceback.format_exception_only(type(e), e))

            # Display and handle test result
            POSSIBLE += points
            if points:
                print("%s" % \
                    (color("red", "FAIL") if fail else color("green", "OK")), end=' ')
            if time.time() - start > 0.1:
                print("(%.1fs)" % (time.time() - start), end=' ')
            print()
            if fail:
                print("    %s" % fail.replace("\n", "\n    "))
            else:
                TOTAL += points
            for callback in run_test.on_finish:
                callback(fail)
            CURRENT_TEST = None

        # Record test metadata on the test wrapper function
        run_test.__name__ = fn.__name__
        run_test.title = title
        run_test.complete = False
        run_test.on_finish = []
        TESTS.append(run_test)
        return run_test
    return register_test

def run_tests():
    """Set up for testing and run the registered test functions."""

    # Handle command line
    global options
    parser = OptionParser(usage="usage: %prog [-v] [filters...]")
    parser.add_option("-v", "--verbose", action="store_true",
                      help="print commands")
    parser.add_option("--color", choices=["never", "always", "auto"],
                      default="auto", help="never, always, or auto")
    (options, args) = parser.parse_args()

    # Start with a full build to catch build errors
    make()

    # Run tests
    limit = list(map(str.lower, args))
    try:
        for test in TESTS:
            if not limit or any(l in test.title.lower() for l in limit):
                test()
        if not limit:
            print("Score: %d/%d" % (TOTAL, POSSIBLE))
    except KeyboardInterrupt:
        pass
    if TOTAL < POSSIBLE:
        sys.exit(1)

class Runner():
    def __init__(self, bin):
        self.bin = bin
        make(self.bin)
  
    def run(self):
        return Popen(self.bin, stdout=PIPE)

    def match(self, r):
        assert_lines_match(check_output(self.bin).decode("UTF-8"), r)

COLORS = {"default": "\033[0m", "red": "\033[31m", "green": "\033[32m"}

def assert_lines_match(text, *regexps, **kw):
    # Check text against regexps
    def assert_lines_match_kw(no=[]):
        return no
    no = assert_lines_match_kw(**kw)
    lines = text.splitlines()
    good = set()
    bad = set()
    for i, line in enumerate(lines):
        if any(re.match(r, line) for r in regexps):
            good.add(i)
            regexps = [r for r in regexps if not re.match(r, line)]
        if any(re.match(r, line) for r in no):
           bad.add(i)
    if not regexps and not bad:
        return
    # We failed; construct an informative failure message
    show = set()
    for lineno in good.union(bad):
        for offset in range(-2, 3):
            show.add(lineno + offset)
    if regexps:
        show.update(n for n in range(len(lines) - 5, len(lines)))
    msg = []
    last = -1
    for lineno in sorted(show):
        if 0 <= lineno < len(lines):
            if lineno != last + 1:
                msg.append("...")
            last = lineno
            msg.append("%s %s" % (color("red", "BAD ") if lineno in bad else
                                color("green", "GOOD") if lineno in good
                                else "    ",
                                lines[lineno]))
    if last != len(lines) - 1:
        msg.append("...")
    if bad:
        msg.append("unexpected lines in output")
    for r in regexps:
        msg.append(color("red", "MISSING") + " '%s'" % r)
    raise AssertionError("\n".join(msg))


def color(name, text):
    if options.color == "always" or (options.color == "auto" and os.isatty(1)):
        return COLORS[name] + text + COLORS["default"]
    return text

MAKE_TIMESTAMP = 0

def pre_make():
    """Delay prior to running make to ensure file mtimes change."""
    while int(time.time()) == MAKE_TIMESTAMP:
        time.sleep(0.1)

def post_make():
    """Record the time after make completes so that the next run of
    make can be delayed if needed."""
    global MAKE_TIMESTAMP
    MAKE_TIMESTAMP = int(time.time())

def make(*target):
    pre_make()
    if Popen(("make",) + target).wait():
        sys.exit(1)
    post_make()

__all__ = ["test", "run_tests", "Runner", "color", "pre_make", "post_make", "make"]