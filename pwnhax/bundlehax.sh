tar cvf Source.tar.gz ./Inputs/Source
cat ./Inputs/pwnhaxExtractor.sh Source.tar.gz > ./Outputs/pwnhax.sh
\rm Source.tar.gz
