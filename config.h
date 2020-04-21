/* calc config */

static const char *ps = ": ";

static int   exit_on_err = 0;
static int   prompt = 0;
static int   quiet  = 0;

static const char *format_default = "%ld\n";
static const char *format_decimal = "%ld\n";
static const char *format_octal   = "%lo\n";
static const char *format_hex     = "%lx\n";
