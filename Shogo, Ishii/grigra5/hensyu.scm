(use srfi-1)

(define lst (call-with-input-file "m10.in"
  (lambda (port)
    (let loop [(lst '())
               (in1 (read port))
               (in2 (read port))]
      (if
        (or
          (eof-object? in1)
          (eof-object? in2))
        lst
        (loop
          (cons
            (list in1 in2)
            lst)
          (read port)
          (read port)))))))

(define minimum
  (let loop [(lst (cdr lst))
             (x (caar lst))
             (y (cadar lst))]
    (if (null? lst)
      (list x y)
      (loop
        (cdr lst)
        (min x (caar lst))
        (min y (cadar lst))))))

(call-with-output-file "m11.in"
  (lambda (port)
    (for-each
      (lambda (item)
        (display (- (car item) (car minimum)) port)
        (display #\space port)
        (display (- (cadr item) (cadr minimum)) port)
        (newline port))
      lst)))

