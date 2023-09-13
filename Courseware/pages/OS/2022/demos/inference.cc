struct Model {
  Vec<int> arch; // 神经网络的 “架构”
  Vec<Vec<Vec<float>>> weight; // 神经元之间的连接
  Vec<Vec<float>> bias; // 神经元自身的状态

  float ReLU(float x) {
    return (x > 0) * x; // Rectified Linear Unit (ReLU)
  }

  int recognize(Image im) {
    int n = arch.size();
    Vec<Vec<float>> outs(n); // 所有神经元的输出
    for (int i = 0; i < arch.size(); i++)
      outs[i].resize(arch[i]);

    for (int i = 0; i < im.size(); i++)
      outs[0][i] = im[i]; // 第一层神经元直接输出图片像素

    for (int i = 1; i < n; i++)
      for (int j = 0; j < arch[i]; j++) {
        float sum = 0;
        for (int k = 0; k < arch[i - 1]; k++)
          sum += weight[i][k][j] * outs[i - 1][k];
        sum += bias[i][j];
        outs[i][j] = ReLU(sum); // 前一层神经元输出加权求和的 ReLU
      }

    int best = 0;
    for (int i = 1; i <= 9; i++)
      if (outs[n - 1][i] > outs[n - 1][best])
        best = i;
    return best;
  }
};
