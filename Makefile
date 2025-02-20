.PHONY: clean All

All:
	@echo "==========Building project:[ ucosR - Debug ]----------"
	@"$(MAKE)" -f  "ucosR.mk" && "$(MAKE)" -f  "ucosR.mk" PostBuild
clean:
	@echo "==========Cleaning project:[ ucosR - Debug ]----------"
	@"$(MAKE)" -f  "ucosR.mk" clean
