/* calc config */

static const char *ps = "λ ";
//static const char *ps = ": ";

static int   exiterr = 0;
static int   quiet   = 0;
static int   prompt  = 0;

static const char *fmt_default = "%ld\n";
static const char *fmt_decimal = "%ld\n";
static const char *fmt_octal   = "%lo\n";
static const char *fmt_hex     = "%lx\n";
