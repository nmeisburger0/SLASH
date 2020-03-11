#include "LSHReservoirSampler.h"

void LSHReservoirSampler::checkTableMemLoad() {
    unsigned int maxx = 0;
    unsigned int minn = _numReservoirs;
    unsigned int tt = 0;
    for (unsigned int i = 0; i < _numTables; i++) {
        if (_tableMemAllocator[i] < minn) {
            minn = _tableMemAllocator[i];
        }
        if (_tableMemAllocator[i] > maxx) {
            maxx = _tableMemAllocator[i];
        }
        tt += _tableMemAllocator[i];
    }

    printf("Node %d Table Mem Usage ranges from %f to %f, average %f\n", _myRank,
           ((float)minn) / (float)_numReservoirsHashed, ((float)maxx) / (float)_numReservoirsHashed,
           ((float)tt) / (float)(_numTables * _numReservoirsHashed));
}

void LSHReservoirSampler::showParams() {
    printf("\n");
    printf("<<< LSHR Parameters >>>\n");
    std::cout << "_rangePow " << _rangePow << "\n";
    std::cout << "_rangePow_Rehashed " << _numSecHash << "\n";
    std::cout << "_numTables " << _numTables << "\n";
    std::cout << "_reservoirSize " << _reservoirSize << "\n";

    std::cout << "_dimension " << _dimension << "\n";
    std::cout << "_maxSamples " << _maxSamples << "\n";
    std::cout << "_numReservoirs " << _numReservoirs << "\n";
    std::cout << "_numReservoirsHashed " << _numReservoirsHashed << "\n";
    std::cout << "_maxReservoirRand " << _maxReservoirRand << "\n";
    std::cout << "_tableMemMax " << _tableMemMax << "\n";
    std::cout << "_tableMemReservoirMax " << _tableMemReservoirMax << "\n";
    std::cout << "_tablePointerMax " << _tablePointerMax << "\n";

    printf("\n");
}

void LSHReservoirSampler::viewTables() {
    for (unsigned int which = 0; which < std::min(_numTables, (unsigned int)DEBUGTB); which++) {
        printf("\n");
        printf("<<< Table %d Content >>>\n", which);
        unsigned int maxResShow = 0;
        for (unsigned int t = 0; t < _numReservoirs; t++) {
            if (_tablePointers[tablePointersIdx(_numReservoirsHashed, t, which, _sechash_a,
                                                _sechash_b)] != TABLE_NULL &&
                maxResShow < DEBUGENTRIES) {
                unsigned int allocIdx = _tablePointers[tablePointersIdx(
                    _numReservoirsHashed, t, which, _sechash_a, _sechash_b)];
                printf("Reservoir %d (%u): ", t,
                       _tableMem[tableMemCtIdx(which, allocIdx, _numReservoirsHashed)]);
                for (unsigned int h = 0; h < std::min(_reservoirSize, (unsigned)DEBUGENTRIES);
                     h++) {
                    printf("%u ",
                           _tableMem[tableMemResIdx(which, allocIdx, _numReservoirsHashed) + h]);
                }
                printf("\n");
                maxResShow++;
            }
        }
        printf("\n");
    }
}

void LSHReservoirSampler::tableContents() {
    unsigned int hashBucketLocation;
    for (int t = 0; t < _numTables; t++) {
        printf("\nNode %d - Table %d\n", _myRank, t);
        for (int b = 0; b < std::min(_numReservoirs, (unsigned)256); b++) {
            hashBucketLocation = _tablePointers[tablePointersIdx(_numReservoirsHashed, b, t,
                                                                 _sechash_a, _sechash_b)];
            if (hashBucketLocation == TABLE_NULL) {
                continue;
            }
            printf("[%d]: ", b);
            for (int i = 0; i < _reservoirSize; i++) {
                if (_tableMem[tableMemResIdx(t, hashBucketLocation, _numReservoirsHashed) + i] ==
                    0) {
                    break;
                }
                printf("%d ",
                       _tableMem[tableMemResIdx(t, hashBucketLocation, _numReservoirsHashed) + i]);
            }
            printf("\n");
        }
        printf("\n");
    }
}
