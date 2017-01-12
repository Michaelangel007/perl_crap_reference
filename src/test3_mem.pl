    open(DATA,"<words.txt") || die "Can't open file";

    my %words;

    while(<DATA>)
    {
      $words{$_} = 1;
    }
    close(DATA);

    print scalar keys %words," unique lines\n";

# 1. OSX doe not support: -o size
# 2. `vsize` is an alias for `vsz`
my $size = `ps -h -p $$ -o pid,rss,vsz`; # or `-o pid -o rss -o vsz`
print $size, "\n"; # 12,972 KB

