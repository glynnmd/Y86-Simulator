#!/bin/bash

rm -f student.output
./lab4 > student.output
diff instructor.output student.output > diffs

if [ -s diffs ]; then
   echo 'Some tests did not pass.'
   echo 'It should produce identical output to the file instructor.output.'
   echo 'To see your output, type this at the shell % prompt: lab4'
   echo 'To see the diffs, type this at the shell % prompt: diff instructor.output student.output'
   echo 'The lines from the instructor.output file begin with <'
   echo 'The lines from the student.output file begin with >'
else
   echo 'All tests passed.'
fi
rm diffs
