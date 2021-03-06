#include "dataset.h"

/** For reading sparse matrix dataset in index:value format.
        fileName - name in string
        offset - which datapoint to start reading, normally should be zero
        n - how many data points to read
        indices - array for storing indices
        values - array for storing values
        markers - the start position of each datapoint in indices / values. It
   have length(n + 1), the last position stores start position of the (n+1)th
   data point, which does not exist, but convenient for calculating the length
   of each vector.
*/

void readSparse(std::string fileName, unsigned int offset, unsigned int n, unsigned int *indices,
                float *values, unsigned int *markers, unsigned int bufferlen) {

    // std::cout << "[readSparse]" << std::endl;

    /* Fill all the markers with the maximum index for the data, to prevent
       indexing outside of the range. */
    for (size_t i = 0; i <= n; i++) {
        markers[i] = bufferlen - 1;
    }

    std::ifstream file(fileName);
    std::string str;

    size_t ct = 0;                  // Counting the input vectors.
    size_t totalLen = 0;            // Counting all the elements.
    while (std::getline(file, str)) // Get one vector (one vector per line).
    {
        if (ct < offset) { // If reading with an offset, skip < offset vectors.
            ct++;
            continue;
        }
        // Constructs an istringstream object iss with a copy of str as content.
        std::istringstream iss(str);
        // Removes label.
        std::string sub;
        iss >> sub;
        // Mark the start location.
        markers[ct - offset] = std::min(totalLen, (size_t)bufferlen - 1);
        int pos;
        float val;
        unsigned int curLen = 0; // Counting elements of the current vector.
        do {
            std::string sub;
            iss >> sub;
            pos = sub.find_first_of(":");
            if (pos == std::string::npos) {
                continue;
            }
            val = stof(sub.substr(pos + 1, (str.length() - 1 - pos)));
            pos = stoi(sub.substr(0, pos));

            if (totalLen < bufferlen) {
                indices[totalLen] = pos;
                values[totalLen] = val;
            } else {
                std::cout << "[readSparse] Buffer is too small, data is truncated!\n";
                return;
            }
            curLen++;
            totalLen++;
        } while (iss);

        ct++;
        if (ct == (offset + n)) {
            break;
        }
    }
    markers[ct - offset] = totalLen; // Final length marker.
    std::cout << "[readSparse] Read " << totalLen << " numbers, " << ct - offset << " vectors. "
              << std::endl;
}

std::streampos readSparse2(std::string fileName, std::streampos fileOffset, unsigned int offset,
                           unsigned int n, unsigned int *indices, float *values,
                           unsigned int *markers, unsigned int bufferlen) {

    // std::cout << "[readSparse]" << std::endl;

    /* Fill all the markers with the maximum index for the data, to prevent
       indexing outside of the range. */

    std::ifstream file(fileName);

    file.seekg(fileOffset);

    for (size_t i = 0; i <= n; i++) {
        markers[i] = bufferlen - 1;
    }

    std::string str;

    size_t ct = 0;                  // Counting the input vectors.
    size_t totalLen = 0;            // Counting all the elements.
    while (std::getline(file, str)) // Get one vector (one vector per line).
    {
        if (ct < offset) { // If reading with an offset, skip < offset vectors.
            ct++;
            continue;
        }
        // Constructs an istringstream object iss with a copy of str as content.
        std::istringstream iss(str);
        // Removes label.
        std::string sub;
        iss >> sub;
        // Mark the start location.
        markers[ct - offset] = std::min(totalLen, (size_t)bufferlen - 1);
        int pos;
        float val;
        unsigned int curLen = 0; // Counting elements of the current vector.
        do {
            std::string sub;
            iss >> sub;
            pos = sub.find_first_of(":");
            if (pos == std::string::npos) {
                continue;
            }
            val = stof(sub.substr(pos + 1, (str.length() - 1 - pos)));
            pos = stoi(sub.substr(0, pos));

            if (totalLen < bufferlen) {
                indices[totalLen] = pos;
                values[totalLen] = val;
            } else {
                std::cout << "[readSparse] Buffer is too small, data is truncated!\n";
                return 0;
            }
            curLen++;
            totalLen++;
        } while (iss);

        ct++;
        if (ct == (offset + n)) {
            break;
        }
    }
    markers[ct - offset] = totalLen; // Final length marker.
    std::cout << "[readSparse] Read " << totalLen << " numbers, " << ct - offset << " vectors. "
              << std::endl;

    std::streampos end = file.tellg();
    file.close();
    return end;
}

void writeTopK(std::string filename, unsigned int numQueries, unsigned int k, unsigned int *topK) {
    std::ofstream file;
    file.open(filename);
    for (size_t q = 0; q < numQueries; q++) {
        for (size_t i = 0; i < k; i++) {
            file << topK[q * k + i] << " ";
        }
        file << "\n";
    }
    file.close();
}

void writeTopK2(std::string filename, unsigned int numQueries, unsigned int k, unsigned int *topK) {
    FILE *file = fopen(filename.c_str(), "a");
    if (file == NULL) {
        printf("Error opening results file.\n");
        exit(1);
    }

    if (fwrite(topK, sizeof(unsigned int), k * numQueries, file) != k * numQueries) {
        printf("Error writing to results file.\n");
        exit(1);
    }
    fclose(file);
}

void readTopK(std::string filename, unsigned int numQueries, unsigned int k, unsigned int *topK) {
    std::ifstream file(filename);
    std::string str;
    unsigned int total = 0;
    while (std::getline(file, str)) {
        std::istringstream iss(str);
        for (size_t i = 0; i < k; i++) {
            std::string item;
            iss >> item;
            topK[total] = stoi(item);
            total++;
        }
    }
    assert(total == numQueries * k);
    printf("Read top %d vectors for %d Queries\n", k, numQueries);
}

void similarityMetric(unsigned int *queries_indice, float *queries_val,
                      unsigned int *queries_marker, unsigned int *bases_indice, float *bases_val,
                      unsigned int *bases_marker, unsigned int *queryOutputs,
                      unsigned int numQueries, unsigned int topk, unsigned int availableTopk,
                      unsigned int *nList, unsigned int nCnt) {

    float *out_avt = new float[nCnt]();

    std::cout << "[similarityMetric] Averaging output. " << std::endl;
    /* Output average. */
    for (size_t i = 0; i < numQueries; i++) {
        unsigned int startA, endA;
        startA = queries_marker[i];
        endA = queries_marker[i + 1];
        for (unsigned int j = 0; j < topk; j++) {
            unsigned int startB, endB;
            startB = bases_marker[queryOutputs[i * topk + j]];
            endB = bases_marker[queryOutputs[i * topk + j] + 1];
            float dist = cosineDist(queries_indice + startA, queries_val + startA, endA - startA,
                                    bases_indice + startB, bases_val + startB, endB - startB);
            for (unsigned int n = 0; n < nCnt; n++) {
                if (j < nList[n])
                    out_avt[n] += dist;
            }
        }
    }

    /* Print results. */
    printf("\nS@k = s_out(s_true): In top k, average output similarity (average "
           "groundtruth similarity). \n");
    for (unsigned int n = 0; n < nCnt; n++) {
        printf("S@%d = %1.3f \n", nList[n], out_avt[n] / (numQueries * nList[n]));
    }
    for (unsigned int n = 0; n < nCnt; n++)
        printf("%d ", nList[n]);
    printf("\n");
    for (unsigned int n = 0; n < nCnt; n++)
        printf("%1.3f ", out_avt[n] / (numQueries * nList[n]));
    printf("\n");
}
