// Copyright (c) 2019, Alibaba Inc.
/* Copyright 2020 Alibaba Group Holding Limited. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "contrib/knn/index.h"
#include "contrib/knn/index_factory.h"
#include "gtest/gtest.h"

namespace graphlearn {
namespace op {

class FlatIndexTest : public ::testing::Test {
protected:
  void SetUp() override {
  }

  void TearDown() override {
  }

  void TestIndex(KnnIndex* index) {
    // Prepare training data
    float* datas = new float[100 * 64];
    int64_t* ids = new int64_t[100];
    for (int i = 0; i < 100; ++i) {
      ids[i] = i * 100;
      float value = static_cast<float>(i) / 100.0;
      for (int j = 0; j < 64; ++j) {
        datas[i * 64 + j] = value;
      }
    }

    index->Train(100, datas);
    index->Add(100, datas, ids);

    delete [] datas;
    delete [] ids;

    // Prepare input data
    int32_t batch_size = 3;
    float* inputs = new float[batch_size * 64];
    for (int i = 0; i < batch_size; ++i) {
      float value = static_cast<float>((i + 1) * 25) / 100.0;
      for (int j = 0; j < 64; ++j) {
        inputs[i * 64 + j] = value;
      }
    }

    int32_t k = 5;
    int64_t* ret_ids = new int64_t[batch_size * k];
    float* ret_distances = new float[batch_size * k];

    index->Search(batch_size, inputs, k, ret_ids, ret_distances);

    // Check results
    for (int i = 0; i < batch_size * k; ++i) {
      int batch_index = i / k;
      int k_index = i % k;
      if (k_index  == 0) {
        std::cout << std::endl;
      }
      std::cout << ret_ids[i] << ' ';
      EXPECT_EQ(abs(ret_ids[i] - 2500 * (batch_index + 1)),
                (k_index + 1) / 2 * 100);
    }
    std::cout << std::endl;

    for (int i = 0; i < batch_size * k; ++i) {
      if (i % k == 0) {
        std::cout << std::endl;
      }
      std::cout << ret_distances[i] << ' ';
    }
    std::cout << std::endl;

    delete [] ret_ids;
    delete [] ret_distances;
  }
};

TEST_F(FlatIndexTest, TrainAddSearch) {
  IndexOption option;
  option.index_type = "flat";
  option.dimension = 64;

  // Get index
  KnnIndex* index = KnnIndexFactory::Create(option);
  ASSERT_TRUE(index != nullptr);

  TestIndex(index);
  delete index;
}

TEST_F(FlatIndexTest, GpuTrainAddSearch) {
  IndexOption option;
  option.index_type = "gpu_flat";
  option.dimension = 64;

  // Get index
  KnnIndex* index = KnnIndexFactory::Create(option);
  ASSERT_TRUE(index != nullptr);

  TestIndex(index);
  delete index;
}

}  // namespace op
}  // namespace graphlearn
