#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "backends/p4tools/common/compiler/context.h"
#include "backends/p4tools/common/lib/logging.h"
#include "frontends/common/options.h"
#include "lib/compile_context.h"
#include "test/gtest/helpers.h"

#include "backends/p4tools/modules/testgen/core/symbolic_executor/path_selection.h"
#include "backends/p4tools/modules/testgen/options.h"
#include "backends/p4tools/modules/testgen/targets/bmv2/test/gtest_utils.h"
#include "backends/p4tools/modules/testgen/testgen.h"

namespace P4::P4Tools::Test {

using namespace P4::literals;

class P4TestgenBenchmark : public P4TestgenBmv2Test {};

TEST_F(P4TestgenBenchmark, SuccessfullyGenerate1000Tests) {
    auto &testgenOptions = P4Testgen::TestgenOptions::get();
    testgenOptions.target = "bmv2"_cs;
    testgenOptions.arch = "v1model"_cs;
    auto includePath = P4CTestEnvironment::getProjectRoot() / "p4include";
    testgenOptions.preprocessor_options = "-I" + includePath.string();
    auto fabricFile =
        P4CTestEnvironment::getProjectRoot() / "testdata/p4_16_samples/fabric_20190420/fabric.p4";
    testgenOptions.file = fabricFile.string();
    testgenOptions.testBackend = "PROTOBUF_IR"_cs;
    testgenOptions.testBaseName = "dummy"_cs;
    testgenOptions.seed = 1;
    // Fix the packet size.
    testgenOptions.minPktSize = 512;
    testgenOptions.maxPktSize = 512;
    // Select a random path for each test.
    testgenOptions.pathSelectionPolicy = P4Tools::P4Testgen::PathSelectionPolicy::RandomBacktrack;
    // Generate 2000 tests.
    testgenOptions.maxTests = 2000;
    // This enables performance printing.
    P4Tools::enablePerformanceLogging();

    auto testList = P4Testgen::Testgen::generateTests(testgenOptions);
    ASSERT_TRUE(testList.has_value());

    // Print the report.
    P4Tools::printPerformanceReport();
}
}  // namespace P4::P4Tools::Test
