import csv
import json
import os
from datetime import date



# Defines the appropriate file names for this file to function properly. 
current_file_name = "appendix/project-summary/current-project.csv"
previous_file_name = "tmp/previous-project.csv"
output_file_name = "appendix/project-summary/project-changes.tex"
project_data_file_name = "tmp/project-data.json"



# Defines the association between the task statuses and their row colors. 
statusToColor = {
    "Backlog": "Ivory2",
    "Todo": "IndianRed1",
    "In Progress": "LightGoldenrod1",
    "Done": "DarkSeaGreen1",
    "\\textbf{\\scriptsize{Status}}": "white" # Used for the case of current-project csv being empty.
}



# This function takes a date formatted as YYYY-MM-DD and returns a date formatted
# as (M)M/(D)D/YYYY to match the other dated entries in the notebook.
def correct_date(date):
    items = date.split('-')
    if items:
        return items[1].lstrip('0') + '/' + items[2].lstrip('0') + '/' + items[0]
    return ""



# This section renames the current project csv file, loads the json for the project date, 
# and loads the json for the GitHub login usernames to first names.
os.rename(current_file_name, previous_file_name)

with open(project_data_file_name) as file:
    project_data = json.load(file)



# This section takes the json information provided by GitHub projects and turns it into a 
# makes it the current csv file for the project while keeping a copy of the previous version. 
current_file = open(current_file_name, 'w', newline = '')
csv_writer = csv.writer(current_file, delimiter='\0')

csv_writer.writerow(["\\textbf{\scriptsize{Labels}}", "\\textbf{\scriptsize{Title}}", "\\textbf{\scriptsize{Status}}", "\\textbf{\scriptsize{Start}}", "\\textbf{\scriptsize{End}}", "\\textbf{\scriptsize{Assignees}}"])

for item in project_data["data"]["organization"]["projectV2"]["items"]["nodes"]:
    row = [''] * 6
    
    labels = []
    for node in item["content"]["labels"]["nodes"]:
        labels.append(node["name"].capitalize())
    row[0] = ", ".join(labels)

    for node in item["fieldValues"]["nodes"]:
        if node:
            match node["field"]["name"]:
                case "Title":
                    row[1] = node["text"]
                case "Status":
                    row[2] = node["name"]
                case "Start":
                    row[3] = correct_date(node["date"])
                case "End":
                    row[4] = correct_date(node["date"])
    assignees = [node["login"] for node in item["content"]["assignees"]["nodes"]]

    row[5] = ", ".join(assignees)
    
    csv_writer.writerow(row)

current_file.close()



# This section compares the lines in the current project csv file with the previous version
# and stores the differences in a list. 
current_file = open(current_file_name, 'r')
previous_file = open(previous_file_name, 'r')

current_lines = current_file.readlines()
previous_lines = previous_file.readlines()
differences = []
for line in current_lines:
    if line not in previous_lines:
        differences.append(line)

current_file.close()
previous_file.close()
os.remove(previous_file_name)



# If differences between the two versions exist, this section writes them out in a LaTeX 
# tabular format to the appropriate output file. 
if differences:
    with open(output_file_name, 'a') as output:
        output.write(f"\n\n\\section{{{date.today().strftime('%-m/%-d/%-Y')} Changes}}\n")
        output.write("\\begin{center}\n")
        output.write("\\begin{longtable}{| p{40pt} | p{150pt} | p{35pt} | p{33pt} | p{33pt} | p{40pt} |}\n")
        for difference in differences:
            columns = difference.split('\0')
            output.write("\\hline ")
            rowColor = statusToColor[columns[2]]
            output.write(f"\\rowcolor{{{statusToColor[columns[2]]}}}\n")
            output.write(' & '.join(columns).strip() + " \\\\\n")
        output.write("\\hline\n")
        output.write("\end{longtable}\n")
        output.write("\\end{center}\n")