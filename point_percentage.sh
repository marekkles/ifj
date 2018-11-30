#TOTAL_LOC=`wc -l $(git ls-files) | tail -n1 |cut -d' '`

TOTAL_LOC=0

IFS=$'\n'

for name in `git log --format='%aN' | sort -u`
do
    loc=`git log --author="$name" --pretty=tformat: --numstat \
|   gawk '{ add += $1; subs += $2; loc += $1 - $2 } END { printf "%s\n", loc }' -`
    ((TOTAL_LOC = TOTAL_LOC + loc))
done


for name in `git log --format='%aN' | sort -u`
do
    loc=`git log --author="$name" --pretty=tformat: --numstat \
|   gawk '{ add += $1; subs += $2; loc += $1 - $2 } END { printf "%s\n", loc }' -`
    expression=`echo scale=3\; $loc/$TOTAL_LOC*100`
    percentage=`bc <<<$expression`
    echo $name $percentage%
done
