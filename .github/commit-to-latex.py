import argparse
import json


# This section sets up the parameters this script uses, such as input, output,
# delimiter, and spacings. Additionally, a list of characters that need to be
# escaped is provided and the json file containing username to real name
# information is given.
escape_characters = ['#'] # Add more if necessary.

parser = argparse.ArgumentParser("csv-to-latex", 
                                 description = "Turns CSV files into LaTeX files.")
parser.add_argument("input", 
                    help = "The CSV file to read data from.", 
                    type = str)
parser.add_argument("output", 
                    help = "The LaTeX file to write to.", 
                    type = str)
parser.add_argument("delimiter",
                    help = "The delimiter for the CSV file.",
                    type = str)
parser.add_argument("spacings", 
                    help = "Spacing for each column as a list of integers.", 
                    type = int, 
                    nargs = '*')
args = parser.parse_args()



# This section will open both the input file in read mode and the output file in
# write mode and proceed to write the appropriate LaTeX to the output file. This
# will also account for characters such as '#' that need to be escaped (i.e.
# '\#') and to change usernames to their real names if available.
with open(args.input, 'r') as input, open(args.output, 'w') as output: 
    output.write("\\begin{center}\n")
    format = [f"p{{{spacing}pt}}" for spacing in args.spacings]
    format = '|' + '|'.join(format) + '|'
    output.write("\\begin{longtable}{" + format + "}\n")
    output.write("\\hline\n")
    for line in input:
        line = [''.join(['\\' + c if c in escape_characters else c for c in value]) for value in line.split(args.delimiter)]
        output.write(' & '.join(line).strip() + " \\\\\n")
        output.write("\\hline\n")
    output.write("\end{longtable}\n")
    output.write("\\end{center}\n")