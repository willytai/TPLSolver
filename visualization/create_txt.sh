# this script will draw the layout by gnuplot and saves the result in html/
# based on the simplified benchmark in ../benchmark/simplified_benchmark/
# if there is nothing in ../benchmark/simplified_benchmark/, run ../benchmark/simplify.sh first

# to see the result, open the html file in html/ with any browser

for pre in c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 s1 s2 s3 s4 s5
do
	if [ ! -f txt/${pre}sim_merged.txt ]; then
		python3 create_txt.py ../benchmark/simplified_benchmark/${pre}sim_merged.ascii
	fi
done

# the place to save html file
if [ ! -d html ]; then
	mkdir html
fi
