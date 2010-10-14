set terminal svg

set size 1,0.75

set output "write-cpu.svg"

set key Right spacing 1.5
set key top

set grid linewidth 1

set xlabel "Dimensione del file"
set ylabel "Tempo (in millesimi di secondo)"

set xrange [1:22]
set yrange [0:200]
set xtics -400, 400, 400
set ytics 0, 40, 200
set xtics rotate by -45

set xtics add ("128b" 1)
set xtics add ("" 2)
set xtics add ("512b" 3)
set xtics add ("" 4)
set xtics add ("2 kb" 5)
set xtics add ("" 6)
set xtics add ("8 kb" 7)
set xtics add ("" 8)
set xtics add ("32 kb" 9)
set xtics add ("" 10)
set xtics add ("128 kb" 11)
set xtics add ("" 12)
set xtics add ("512 kb" 13)
set xtics add ("" 14)
set xtics add ("2 Mb" 15)
set xtics add ("" 16)
set xtics add ("8 Mb" 17)
set xtics add ("" 18)
set xtics add ("32 Mb" 19)
set xtics add ("" 20)
set xtics add ("128 Mb" 21)
set xtics add ("" 22)


plot "write-cpu.dat" using 1:2 notitle with linespoints linewidth 2 pointtype 9 linetype 2
