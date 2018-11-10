echo "starting test";
# output from MDT -> remove lines starting with # -> compare with answer
build/MSAX -i test/insect_b.txt -f 10 -a 4 -w 100   | cmp test/test_sequence_f_10_a_4_w_100.answer;
build/MSAX -i test/insect_b.txt -f 100 -a 4 -w 100  | cmp test/test_sequence_f_100_a_4_w_100.answer;
build/MSAX -i test/insect_b.txt -f 1000 -a 8 -w 100 | cmp test/test_sequence_f_1000_a_8_w_100.answer;
build/MSAX -i test/insect_b.txt -f 20 -a 16 -w 200  | cmp test/test_sequence_f_20_a_16_w_200.answer;
echo "test completed";
