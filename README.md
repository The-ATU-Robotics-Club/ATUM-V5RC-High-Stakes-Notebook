# ATUM VRC Over Under Notebook
Team ATUM's notebook for the 2024-2025 V5RC game, High Stakes. For judges, this
repository contains the entire version history of the notebook. For ATUM team
members, below is a short guide to contributing and working with this notebook.
See the README on the organization page for information regarding GitHub in
general. 



## Set Up
To contribute to the notebook, you must be invited as a contributor to the 
Overleaf. This can be done by following this [link](https://www.overleaf.com/1325617833kjhsnwskxphg#a396fc),
though there is a limited number of contributors, so please only invite yourself
if you intend to work on the notebook. A view-only link (that anyone can use)
can be found [here](https://www.overleaf.com/read/xndcqyrtgghb#903fad).



## Working with Automated Workflows
There are four GitHub workflows that can be seen under the Actions tab on the
GitHub repository page. These are:
* Club Meeting Notes: add all meeting notes in the organization repo to the
  appendix
* Commit Logs: add the commit logs of the notebook, CAD, and code to the
  appendix
* Project Summary: add daily changes to the project management to the appendix
* Full Code: adds all of the code files from the code repo into the software
* section

The project summary action will run automatically each day, and all workflows
can be run manually by going to the Actions tab, clicking on the desired action,
and clicking "Run workflow". 



## Working with LaTeX
LaTeX is a typesetting program that is commonly used for technical writing.
LaTeX documents are produced in a programmatic fashion, with raw text stored in
`.tex` files that are compiled into a format like `.pdf`. Most of the formatting
and in-LaTeX commands have already been implemented and Overleaf provides many 
other commands as macro buttons, so only a handful need to be learned. 

These important commands include: 
* `% Text after a percent sign are comments and will not appear in the final
  pdf`
  * While this may seem useless at first, sometimes LaTeX can be obtuse and need
    additional explanation, so this can be useful.
* ```
  This is a sentence. \\
  This sentence will appear on another line. \linebreak
  This will ALSO appear on another line.
  ```
  and
  ```
  This is a paragraph. \par
  This will be another paragraph.

  This will ALSO appear as another paragraph.
  ```
* `\#` 
  * This produces a pound sign. *Other symbols may need the '\\' as a prefix in
    order to appear.*
* `\qrcode{www.linktoleadto.com}` and `\qrcode[height =
  .5\linewidth]{linktoleadto}` (the latter manipulates the size of the QR code)
  
For a more in-depth tutorial, [Overleaf provides a decent, concise
introduction.](https://www.overleaf.com/learn/latex/Learn_LaTeX_in_30_minutes)
Further, more information can be found in the tutorial presentation in this
repository. 



## Working with the Notebook
* Whenever you are finished with your changes in a notebooking session, click
* "Menu" followed by "GitHub" and "Push Overleaf changes to GitHub." Leave a
* useful, consise commit message (*think: these will appear in the notebook*).
* 
* Dates should be in the format (m)m/(d)d/yyyy such as 1/1/2023 or 12/31/2023
* Files and directories should be all lowercase and spaces should be separated
  with dashes (-)
* Images should be placed in the central images directory, in the appropriate
  month subdirectory, and with a precise name.
  
* When adding a new chapter:
  * **Each chapter should be placed in its own file**
  * Each chapter file should be placed in its own similarly-named directory
  * Sections may be broken into their own files and inserted into the main
    chapter file
  * In order to add the chapter to the main notebook, the chapter file must be
    inserted into the appropriate part file as so, i.e., a line like
    `\input{part/chapter/chapter.tex}` will be placed into `part/part.tex`
* To summarize the above point, the structure appears as: 
  > notebook
  >> images <br>
  >> design-matrices <br>
  >> part
  >>> chapter
  >>>> chapter.tex <br>
       big-section-a.tex <br>
       big-section-b.tex <br>
  >>>>> image-a.png <br>
        image-b.png 
* **Reference the other notebook files and directories when in doubt**
