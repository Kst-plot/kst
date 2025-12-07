# SPDX-FileCopyrightText: 2025 Carl Schwan <carlschwan@kde.org>
# SPDX-License-Identifier: BSD-2-Clauses

find_package(Qt6 ${QT_MIN_VERSION} NO_MODULE COMPONENTS
    Test
)

ecm_add_tests(
    testcsd.cpp
    #testdatamatrix.cpp
    #testdatasource.cpp
    testeditablematrix.cpp
    testeqparser.cpp
    testgeneratedmatrix.cpp
    testgeneratedvector.cpp
    testhistogram.cpp
    #testlabelparser.cpp
    testmatrix.cpp
    testobjectstore.cpp
    #testpsd.cpp
    testscalar.cpp
    testvector.cpp
    LINK_LIBRARIES
        Kst6Core
        Kst6Math
        Qt::Test
)
