https://stackoverflow.com/questions/16604389/extract-substrings-of-a-filename/16605408#16605408


int FindParts(const char* source, char** firstOut, char** secondOut)
{
const char* last        = NULL;
const char* previous    = NULL;
const char* middle      = NULL;
const char* middle1     = NULL;
const char* middle2     = NULL;
char* first;
char* second;

last = strrchr(source, '\\');
if (!last || (last  == source))
    return -1;
--last;
if (last == source)
    return -1;

previous = last;
for (; (previous != source) && (*previous != '\\'); --previous);
++previous;

{
    middle = strchr(previous, '-');
    if (!middle || (middle > last))
        return -1;

    middle1 = middle-1;
    middle2 = middle+1;
}

//  now skip spaces

for (; (previous != middle1) && (*previous == ' '); ++previous);
if (previous == middle1)
    return -1;
for (; (middle1 != previous) && (*middle1 == ' '); --middle1);
if (middle1 == previous)
    return -1;
for (; (middle2 != last) && (*middle2 == ' '); ++middle2);
if (middle2 == last)
    return -1;
for (; (middle2 != last) && (*last == ' '); --last);
if (middle2 == last)
    return -1;

first   = (char*)malloc(middle1-previous+1 + 1);
second  = (char*)malloc(last-middle2+1 + 1);
if (!first || !second)
{
    free(first);
    free(second);
    return -1;
}

strncpy(first, previous, middle1-previous+1);
first[middle1-previous+1] = '\0';
strncpy(second, middle2, last-middle2+1);
second[last-middle2+1] = '\0';

*firstOut   = first;
*secondOut  = second;

return 1;
}
