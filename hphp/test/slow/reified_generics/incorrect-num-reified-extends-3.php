<?hh

class C<reify Ta, reify Tb> {}
class D extends C<reify int> {}

new D();
