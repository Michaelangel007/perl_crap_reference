open(DATA,"<words.txt") || die "Can't open file";

my %words;

while(<DATA>)
{
  $words{$_} = 1;
}
close(DATA);

print scalar keys %words," unique lines\n";

