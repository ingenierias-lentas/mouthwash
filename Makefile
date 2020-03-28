
.PHONY: mouthwash
mouthwash: conan
	mkdir -p build && cd build && cmake .. -G Ninja && ninja

conan:
	mkdir -p build && cd build && conan install ..
	cd build && conan info .. --graph=conan_graph.html

.PHONY: docs
docs:
	doxygen docs/Doxyfile

.PHONY: clean
clean:
	rm -rf build/
