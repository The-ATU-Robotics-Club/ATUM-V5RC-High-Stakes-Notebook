% ~~~~~~~~~~ CLASS SET UP ~~~~~~~~~~ %
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{ATUEngineeringNotebook}[ATU Engineering Notebook]
\LoadClass[12pt, openany]{book}



% ~~~~~~~~~~ REQUIRED PACKAGES ~~~~~~~~~~ %
\RequirePackage[
    letterpaper,
    portrait,
    margin=1in,
    headheight=15pt,
    headsep=15pt]{geometry} % Used for setting margins.
\RequirePackage[
    backend = biber,
    style = alphabetic,
    sorting = ynt]{biblatex} % Used for interfacing with the bibliography.
\RequirePackage{hyperref} % Used to configure link styling.
\RequirePackage[x11names, table]{xcolor} % Used for wider collection of preset colors.
\RequirePackage{graphicx} % Used for adding images.
\RequirePackage[font=large, labelfont=bf]{caption} % Used to change captions.
\RequirePackage{fancyhdr} % Used to customize headers and footer.
\RequirePackage[]{titlesec} % Used to stylize titles for parts, chapters, and sections.
\RequirePackage[]{tabularx} % Used for tables.
\RequirePackage[]{longtable} % Used for long tables that should continue onto the next page.
\RequirePackage[]{lscape} % Used for sideways sections of the notebook.
\RequirePackage[]{qrcode} % Used for adding QR codes. 
\RequirePackage[]{gensymb} % Used for general symbols such as degrees.
\RequirePackage[]{amssymb} % Used for general symbols such as approximately equal.
\RequirePackage[]{float} % Used for additional options regarding floating objects.
\RequirePackage[]{listings} % Used for highlighted code sections.
\RequirePackage[]{multicol} % Used for supporting multi-column item sections.\usepackage{pdfpages}
\RequirePackage[]{colortbl} % Used for coloring table rows. 
\RequirePackage[figurewithin=chapter]{caption} % Used for changing how figures are counted.
\RequirePackage[]{ulem} % Used for strike-through text. 
\RequirePackage[]{emoji} % Used for emojis.


% ~~~~~~~~~~ GLOBAL DEFINITIONS ~~~~~~~~~~ %
\gdef\season{2024-2025}
\gdef\game{High Stakes}
\gdef\teamname{ATUM's Family}
\gdef\teamnumber{ATUM}
\gdef\matrixwidth{.9}
\gdef\defaulttocdepth{2}
\colorlet{Gold}{Gold1}
\colorlet{Green}{Green4}
\colorlet{MatrixColor}{Green4}



% ~~~~~~~~~~ COMMANDS ~~~~~~~~~~ %
\newcommand{\mypart}[3]{ % Input: {part name} {word to use in place of "chapter"} {part file location}
    \renewcommand{\chaptername}{#2}
    \part{#1}
    \input{#3}
}

\renewcommand{\appendix}[1]{ % Input: {appendix name}
    \stepcounter{chapter}
    \setcounter{section}{0}
    \chapter{#1}
}

\newenvironment{centered}{
    \setlength\topsep{-5pt}
    \setlength\parskip{0pt}
    \begin{center}
        }{
    \end{center}
}

\newcommand{\leftsidemember}[4]{ % Input: {name} {photo file location} {quote} {bio}
    \begin{minipage}{\textwidth}
        \noindent{\color{Green} \rule{\linewidth}{1pt}} \vspace{-25pt} \\
        \begin{centered}
            \textit{"#3"} -- \textbf{\large{#1}}
        \end{centered}
        \noindent{\color{Green} \rule{\linewidth}{1pt}} \vspace{-10pt} \\
        \begin{minipage}{0.2375\textwidth}
            \includegraphics[width=\linewidth]{#2}
        \end{minipage}
        \hfill
        \begin{minipage}{0.75\textwidth}
            \raggedleft
            \footnotesize
            #4
        \end{minipage}
    \end{minipage}
    \vspace{10pt}
}

\newcommand{\rightsidemember}[4]{ % Input: {name} {photo file location} {quote} {bio}
    \begin{minipage}{\textwidth}
        \noindent{\color{Gold} \rule{\linewidth}{1pt}} \vspace{-25pt} \\
        \begin{centered}
            \textit{"#3"} -- \textbf{\large{#1}}
        \end{centered}
        \noindent{\color{Gold} \rule{\linewidth}{1pt}} \vspace{-10pt} \\
        \begin{minipage}{0.75\textwidth}
            \raggedright
            \footnotesize
            #4
        \end{minipage}
        \hfill
        \begin{minipage}{0.2375\textwidth}
            \includegraphics[width=\linewidth]{#2}
        \end{minipage}
    \end{minipage}
    \vspace{10pt}
}


% ~~~~~~~~~~ STYLING ~~~~~~~~~~ %
\let\oldheadrule\headrule % Necessary to change header line color. 
\let\oldfootnoterule\footnoterule % Necessary to change footer line color. 

\fancypagestyle{plain}{
\fancyhf[]{}
\fancyhf[EHL]{\textbf{\color{Green}{\teamnumber}}}
\fancyhf[OHR]{\textbf{\color{Green}{VRC {\season}: {\game}}}}
\fancyhf[EFL]{\textbf{\color{Green}{\thepage}}}
\fancyhf[OFR]{\textbf{\color{Green}{\thepage}}}
\renewcommand{\headrulewidth}{2pt}
\renewcommand{\headrule}{\color{Gold}\oldheadrule}
\renewcommand{\footnoterule}{\color{Gold}\oldfootnoterule}
}

\titleformat{\part}
[block]
{\center\huge\scshape}
{{\partname} {\thepart}:}
{0pt}
{ }

\titleformat{\chapter}
[block]
{\normalfont\LARGE\bf}
{{\chaptername} {\thechapter}: }
{0pt}
{ }

\titleformat{\section}
[block]
{\normalfont\Large\bf}
{ }
{0pt}
{ }

\titlespacing\chapter{0pt}{0pt}{0pt}
\titlespacing\section{0pt}{0pt}{0pt}
\titlespacing\subsection{0pt}{0pt}{0pt}
\titlespacing\subsubsection{0pt}{0pt}{0pt}

\renewcommand{\thesection}{}
\renewcommand{\thesubsection}{}
\renewcommand{\thesubsubsection}{}

\@addtoreset{chapter}{part}

\lstdefinestyle{codestyle}{
    backgroundcolor=\color{white},
    commentstyle=\color{Gray0},
    keywordstyle=\color{Green},
    numberstyle=\color{Gold},
    stringstyle=\color{Gold},
    basicstyle=\ttfamily\footnotesize,
    breakatwhitespace=false,
    breaklines=true,
    captionpos=b,
    keepspaces=true,
    numbers=left,
    numbersep=5pt,
    showspaces=false,
    showstringspaces=false,
    showtabs=false,
    tabsize=2
}

\lstset{style=codestyle}

\renewcommand\p@figure{Figure }



% ~~~~~~~~~~ PENALTIES ~~~~~~~~~~ %
% Thank you BLRS!
\doublehyphendemerits=10000
\brokenpenalty=10000
\widowpenalty=9999
\clubpenalty=9999
\interfootnotelinepenalty=9999



% ~~~~~~~~~~ PDF GENERATION SET UP ~~~~~~~~~~ %
\hypersetup{
    colorlinks=true,
    linkcolor=Green,
    citecolor=Green,
    urlcolor=Gold
}