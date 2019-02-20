set terminal canvas                             # set output terminal as html
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'c7_result.html'
plot\
'c7_red.txt' with filledcurves lc rgb "gray",\
'c7_red.txt' using 1:2 with lines lc rgb "gray",\
'c7_green.txt' with filledcurves lc rgb "green",\
'c7_green.txt' using 1:2 with lines lc rgb "green",\
'c7_blue.txt' with filledcurves lc rgb "blue",\
'c7_blue.txt' using 1:2 with lines lc rgb "blue",\
'c7_conflict.txt' with filledcurves lc rgb "red",\
'c7_conflict.txt' using 1:2 with lines lc rgb "red"
