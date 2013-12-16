options(stringsAsFactor=FALSE)

library(rjson)
library(plyr)
library(ggplot2)
library(reshape2)

results <- ldply(fromJSON(file="results.json"), unlist)
