set terminal canvas                             # set output terminal as html
set style fill transparent solid 0.5 noborder   # set fill style
unset xtics                                     # remove x lables
unset ytics                                     # remove y labes
set nokey                                       # remove legend
set output 'html/c1sim_merged.html'
plot 'txt/c1sim_merged.txt' with filledcurves lc rgb "red", 'txt/c1sim_merged.txt' using 1:2 with lines lc rgb "red"
