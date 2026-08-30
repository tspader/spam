#include <git2.h>

#include <stdio.h>

static int fail(const char* what) {
  const git_error* err = git_error_last();
  fprintf(stderr, "%s: %s\n", what, err ? err->message : "unknown error");
  return 1;
}

int main(int argc, char** argv) {
  const char* path = argc > 1 ? argv[1] : "init-example-repo";
  git_repository* repo = NULL;
  git_index* index = NULL;
  git_tree* tree = NULL;
  git_signature* sig = NULL;
  git_oid tree_id;
  git_oid commit_id;
  char hex[GIT_OID_SHA1_HEXSIZE + 1];
  int major, minor, rev;

  git_libgit2_init();
  git_libgit2_version(&major, &minor, &rev);
  printf("libgit2 %d.%d.%d features=0x%x\n", major, minor, rev, git_libgit2_features());

  if (git_repository_init(&repo, path, 0) < 0) return fail("init");
  if (git_repository_index(&index, repo) < 0) return fail("index");
  if (git_index_write_tree(&tree_id, index) < 0) return fail("write-tree");
  if (git_tree_lookup(&tree, repo, &tree_id) < 0) return fail("tree");
  if (git_signature_now(&sig, "spn", "spn@example.com") < 0) return fail("signature");
  if (git_commit_create_v(&commit_id, repo, "HEAD", sig, sig, NULL, "initial", tree, 0) < 0) return fail("commit");

  git_oid_tostr(hex, sizeof(hex), &commit_id);
  printf("%s %s\n", hex, path);

  git_signature_free(sig);
  git_tree_free(tree);
  git_index_free(index);
  git_repository_free(repo);
  git_libgit2_shutdown();
  return 0;
}
