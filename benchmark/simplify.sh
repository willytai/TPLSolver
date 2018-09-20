for i in 1 2 3 4 5 6 7 8 9 10
do
	echo simplifing c${i}sim_merged.ascii...
	python3 simplify.py c${i}sim_merged.ascii
done

for i in 1 2 3 4 5
do
	echo simplifing s${i}sim_merged.ascii...
	python3 simplify.py s${i}sim_merged.ascii
done
