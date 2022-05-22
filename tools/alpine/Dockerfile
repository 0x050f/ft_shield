FROM alpine:latest as alpine-test

RUN apk update &&		\
	apk add --no-cache	\
		bash			\
		build-base		\
		gcc				\
		make			\
		openrc

WORKDIR app/
