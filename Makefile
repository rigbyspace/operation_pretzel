PYTHON ?= python3

.PHONY: theorist

theorist:
	$(PYTHON) theorist_gpt.py --scan-all --suggest-tests --report
