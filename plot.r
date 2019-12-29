plot <- function(row)
{
    data <- read.table(file = "~/Documents/csp/distr.txt", header=T, row.names = 1)
    xaxis <- colnames(data)
    yaxis <- as.numeric(data[toString(row),])
    barplot(height = yaxis/sum(yaxis), names.arg = xaxis, main = paste("node", row))
}
