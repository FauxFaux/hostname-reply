FROM alpine
WORKDIR /b
RUN apk add --update gcc libc-dev
ADD app.c ./
RUN gcc -O1 -static app.c

FROM scratch
COPY --from=0 /b/a.out /a
CMD ["/a"]
