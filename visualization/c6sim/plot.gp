set terminal canvas                             # set output terminal as html
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'c6_result.html'
plot\
'c6_red.txt' with filledcurves lc rgb "gray",\
'c6_red.txt' using 1:2 with lines lc rgb "gray",\
'c6_green.txt' with filledcurves lc rgb "green",\
'c6_green.txt' using 1:2 with lines lc rgb "green",\
'c6_blue.txt' with filledcurves lc rgb "blue",\
'c6_blue.txt' using 1:2 with lines lc rgb "blue",\
'c6_conflict.txt' with filledcurves lc rgb "red",\
'c6_conflict.txt' using 1:2 with lines lc rgb "red"
