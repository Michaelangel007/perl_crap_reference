open(DATA,"<words.txt") || die "Can't open file";

while(<DATA>)
{
  $lines++;
}
close(DATA);

print "$lines lines\n";

