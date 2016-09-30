function Result = ImageDownResolution(A, blocksize)
[rows, cols] = size(A);                              % the input matrix A is viewed as
result_rows = rows/blocksize;                        % a series of square blocks
result_cols = cols/blocksize;                        % of size = blocksize
Result = zeros([result_rows result_cols]);
for crt_row = 1:result_rows                          % then each pixel is computed as
   for crt_col = 1:result_cols                       % the average of each such block
      Result(crt_row, crt_col) = mean2(A(1+(crt_row-1)*blocksize:crt_row*blocksize, ...
                                       1+(crt_col-1)*blocksize:crt_col*blocksize));
   end
end

