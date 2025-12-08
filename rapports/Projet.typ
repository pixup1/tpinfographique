#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-document(
    "light",
    cover-top-left: [
        #set text(27pt)
        *Informatique Graphique - Projet*
        #set text(24pt)
        Tortue Kaizen
    ],
    cover-middle-left: [
        #v(-70pt)
        *GRAINDORGE Amance*
        
        *VERNET Hector*
    ],
    cover-bottom-right: "",
    page-header: [
        Projet d'Informatique Graphique
        #h(1fr)
        15/12/2025
    ],
    doc,
)

#show link: it => underline(offset: 2pt)[#it]

#show raw.where(block: true): it => {
    if it.lines.first().count > 1 {
        block(stroke: 0.5pt + black, inset: 5pt, width: 100%, grid(
            columns: (auto, auto),
            column-gutter: 2em,
            row-gutter: par.leading,
            align: (right, raw.align),
            ..for line in it.lines {
                (
                    text(fill: luma(150))[#line.number],
                    line.body,
                )
            },
        ))
    } else { it.lines.first() }
}

= Introduction

Dans ce rapport, nous allons détailler la réalisation de notre projet d'informatique graphique et plus spécifiquement, les éléments techniques que nous avons ajouté au code fourni pour obtenir le résultat final.

= Animation

= Matériaux avec transparence

= Post-processing

= Simulation

= Conclusion