#import "@preview/silky-report-insa:0.5.2": *
#show: doc => insa-report(
  id: 1,
  pre-title: "Informatique Graphique",
  title: "Démarrage",
  authors: [
    *GRAINDORGE Amance*
    
    *VERNET Hector*
    #v(70pt)
  ],
  date: "20/10/2025",
  insa: "rennes",
  doc,
)

= Introduction

Le but de ce premier TP est d'installer la pipeline fournie par les encadrants et de commencer à s'en servir. On ira jusqu'à afficher un cube coloré et lui appliquer des transformations.